// Навигация. Прямое нарушение YAGNI:
// ссылки на /about, /users/123 и /products добавлены "на будущее",
// хотя таких страниц в приложении нет и не планируется.

function createNavigation() {
  const nav = document.createElement("nav");
  const ul = document.createElement("ul");

  // Главная — единственная реально существующая страница.
  const homeLi = document.createElement("li");
  const homeLink = document.createElement("a");
  homeLink.href = "/";
  homeLink.textContent = "Главная";
  homeLink.className = isActive("/") ? "active" : "";
  homeLi.appendChild(homeLink);
  ul.appendChild(homeLi);

  // ---- НАРУШЕНИЕ YAGNI ----
  // Страницы /about нет. Ссылка добавлена на будущее.
  const aboutLi = document.createElement("li");
  const aboutLink = document.createElement("a");
  aboutLink.href = "/about";
  aboutLink.textContent = "О нас";
  aboutLink.className = isActive("/about") ? "active" : "";
  aboutLi.appendChild(aboutLink);
  ul.appendChild(aboutLi);

  // Страницы /users/123 нет. Ссылка добавлена на будущее.
  const userLi = document.createElement("li");
  const userLink = document.createElement("a");
  userLink.href = "/users/123";
  userLink.textContent = "Пользователь 123";
  userLi.appendChild(userLink);
  ul.appendChild(userLi);

  // Страницы /products нет. Ссылка добавлена на будущее.
  const productsLi = document.createElement("li");
  const productsLink = document.createElement("a");
  productsLink.href = "/products";
  productsLink.textContent = "Продукты";
  productsLi.appendChild(productsLink);
  ul.appendChild(productsLi);
  // ---- КОНЕЦ НАРУШЕНИЯ ----

  nav.appendChild(ul);
  return nav;
}

function isActive(path) {
  return window.location.pathname === path;
}

document.body.appendChild(createNavigation());
