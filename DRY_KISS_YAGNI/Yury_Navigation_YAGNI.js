import { Link, NavLink } from 'react-router-dom'

const Navigation = () => {
  return (
    <nav>
      <ul>
        <li>
          <NavLink 
            to="/" 
            end
            className={({ isActive }) => isActive ? 'active' : ''}
          >
            Главная
          </NavLink>
        </li>
        // Прямое нарушение YAGNI, так как страниц /about, /users/123 и /products нет.
        // Код был добавленн на будущие
        <li>
          <NavLink 
            to="/about"
            className={({ isActive }) => isActive ? 'active' : ''}
          >
            О нас
          </NavLink>
        </li>
        <li>
          <Link to="/users/123">Пользователь 123</Link>
        </li>
        <li>
          <Link to="/products">Продукты</Link>
        </li>
      </ul>
    </nav>
  )
}

export default Navigation