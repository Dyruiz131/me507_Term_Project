import { useState, useEffect } from "react";
import Chessboard from "chessboardjsx";

export default function Home() {
  //State variable to store all the external recipes
  const [board, setBoard] = useState<string>();
  const [Chess, setChess] = useState<Chessboard>();

  useEffect(() => {
    fetch("http://localhost:3001/fen")
      .then((res) => res.json())
      .then((data) => setBoard(data.fen));
    setChess(new Chessboard({ position: board }));
  }, [board]);

  function handleMove(from: string, to: string) {
    fetch("http://localhost:3001/move", {
      method: "POST",
      body: JSON.stringify({ from: from, to: to }),
    });
  }
  function handlePieceClick(){

  }

  return (
    <main>
      <h1>Play</h1>
      <Chessboard position={board} onDrop={handleDrop}/>
    </main>
  );
}
