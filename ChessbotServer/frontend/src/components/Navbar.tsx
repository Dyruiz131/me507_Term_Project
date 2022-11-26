import React from "react";
import { Link } from "react-router-dom";

//Navbar that is rendered on each page. Uses react router links.
export default function Navigation() {
  return (
    <header className="header">
      <h1>
        <Link to="/">Chessbot</Link>
      </h1>
      <p>An ME507 project</p>
    </header>
  );
}
