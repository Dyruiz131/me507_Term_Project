import React from "react";
import { Link } from "react-router-dom";
import "./Navbar.css";

//Navbar that is rendered on each page. Uses react router links.
export default function Navigation() {
  return (
    <header className="header">
      <h1>
        <Link to="/">Chessbot</Link>
      </h1>
      <p>An ME507 project</p>
      <nav>
        <Link to="/">Home</Link>
        <Link to="/about">About</Link>
      </nav>
    </header>
  );
}
