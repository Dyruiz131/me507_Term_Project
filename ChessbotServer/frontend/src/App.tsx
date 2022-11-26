import "./App.css";
import Navbar from "./components/Navbar";
import Home from "./components/Home";
// import Board from "./components/Board";
import { BrowserRouter, Route, Routes } from "react-router-dom";

function App() {
  return (
    // BrowswerRouter to control the website navigation
    <BrowserRouter>
      {/* Always render Navbar component */}
      <Navbar />
      <Routes>
        <Route path="/" element={<Home />} />
      </Routes>
    </BrowserRouter>
  );
}

export default App;
