#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

class DeploymentPlanBuilder;

class DeploymentPlan {
public:
    void print() const {
        std::cout << service_ << " -> " << environment_
                  << ", artifact: " << artifact_
                  << ", rollout stages: " << rollout_percentages_.size()
                  << ", rollback: " << (rollback_command_.empty() ? "no" : "yes")
                  << '\n';
    }

private:
    friend class DeploymentPlanBuilder;

    std::string service_;
    std::string environment_;
    std::string artifact_;
    std::vector<std::string> prechecks_;
    std::string migration_script_;
    std::vector<int> rollout_percentages_;
    std::vector<std::string> smoke_tests_;
    std::string rollback_command_;
    std::vector<std::string> notification_channels_;
    bool require_manual_approval_ = false;
};

class DeploymentPlanBuilder {
public:
    DeploymentPlanBuilder(
        std::string service,
        std::string environment,
        std::string artifact
    ) {
        plan_.service_ = std::move(service);
        plan_.environment_ = std::move(environment);
        plan_.artifact_ = std::move(artifact);
    }

    DeploymentPlanBuilder& add_precheck(std::string check) {
        plan_.prechecks_.push_back(std::move(check));
        return *this;
    }

    DeploymentPlanBuilder& run_migration(std::string script) {
        plan_.migration_script_ = std::move(script);
        return *this;
    }

    DeploymentPlanBuilder& rollout_in_stages(std::vector<int> percentages) {
        plan_.rollout_percentages_ = std::move(percentages);
        return *this;
    }

    DeploymentPlanBuilder& add_smoke_test(std::string test) {
        plan_.smoke_tests_.push_back(std::move(test));
        return *this;
    }

    DeploymentPlanBuilder& rollback_with(std::string command) {
        plan_.rollback_command_ = std::move(command);
        return *this;
    }

    DeploymentPlanBuilder& notify(std::string channel) {
        plan_.notification_channels_.push_back(std::move(channel));
        return *this;
    }

    DeploymentPlanBuilder& require_manual_approval() {
        plan_.require_manual_approval_ = true;
        return *this;
    }

    DeploymentPlan build() const {
        validate();
        return plan_;
    }

private:
    DeploymentPlan plan_;

    void validate() const {
        if (plan_.service_.empty() || plan_.environment_.empty() || plan_.artifact_.empty()) {
            throw std::invalid_argument("service, environment and artifact are required");
        }
        if (plan_.rollout_percentages_.empty() ||
            plan_.rollout_percentages_.back() != 100) {
            throw std::invalid_argument("rollout must finish at 100 percent");
        }
        if (!std::is_sorted(
                plan_.rollout_percentages_.begin(),
                plan_.rollout_percentages_.end())) {
            throw std::invalid_argument("rollout percentages must be ordered");
        }
        if (plan_.environment_ == "production") {
            if (plan_.rollback_command_.empty()) {
                throw std::invalid_argument("production deployment requires rollback");
            }
            if (!plan_.require_manual_approval_) {
                throw std::invalid_argument("production deployment requires approval");
            }
            if (plan_.smoke_tests_.empty()) {
                throw std::invalid_argument("production deployment requires smoke tests");
            }
        }
    }
};

// Director хранит только действительно повторяемые стандартные рецепты.
class DeploymentPlanDirector {
public:
    static DeploymentPlan staging(
        const std::string& service,
        const std::string& artifact
    ) {
        return DeploymentPlanBuilder(service, "staging", artifact)
            .add_precheck("config is valid")
            .rollout_in_stages({100})
            .add_smoke_test("GET /health returns 200")
            .build();
    }

    static DeploymentPlan production(
        const std::string& service,
        const std::string& artifact,
        const std::string& previous_artifact,
        const std::string& migration_script
    ) {
        return DeploymentPlanBuilder(service, "production", artifact)
            .add_precheck("config is valid")
            .add_precheck("error budget is available")
            .run_migration(migration_script)
            .rollout_in_stages({5, 25, 50, 100})
            .add_smoke_test("GET /health returns 200")
            .add_smoke_test("test payment succeeds")
            .rollback_with("deploy " + previous_artifact)
            .notify("payments-team")
            .notify("support")
            .require_manual_approval()
            .build();
    }
};

int main() {
    const DeploymentPlan staging = DeploymentPlanDirector::staging(
        "payments",
        "payments:2.4.0"
    );

    const DeploymentPlan production = DeploymentPlanDirector::production(
        "payments",
        "payments:2.4.0",
        "payments:2.3.1",
        "migrations/2026_10_add_payment_status.sql"
    );

    staging.print();
    production.print();
}
