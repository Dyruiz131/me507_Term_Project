import { Chessboard } from "react-chessboard";
import { useState, useEffect } from "react";

export default function Home() {
  const [fen, setFen] = useState(
    "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
  );
  const [feedback, setFeedback] = useState("Make a move!");
  const [turn, setTurn] = useState("w");

  interface move {
    from: string;
    to: string;
    promotion?: string;
  }
  /* Fetch current board on page load */
  useEffect(() => {
    fetch("http://localhost:3001/fen")
      .then((res) => res.json())
      .then((data) => {
        setFen(data.fen);
        setTurn(data.turn);
      });
    if (turn === "w") {
      setFeedback("White's turn");
    } else {
      setFeedback("Black's turn");
    }
  }, [turn]);

  /* Fetch current board every 2s */
  useEffect(() => {
    const interval = setInterval(() => {
      fetch("http://localhost:3001/fen")
        .then((res) => res.json())
        .then((data) => {
          setFen(data.fen);
          setTurn(data.turn);
        });
      if (turn === "w") {
        setFeedback("White's turn");
      } else {
        setFeedback("Black's turn");
      }
    }, 2000); //Fetch board status every 2s

    return () => clearInterval(interval);
  }, [turn]);

  /* Send move to api */
  function sendMove(move: move) {
    fetch("http://localhost:3001/move", {
      method: "POST",
      body: JSON.stringify(move),
      headers: {
        "Content-Type": "application/json",
      },
    })
      .then((res) => res.json())
      .then((data) => {
        setFen(data.fen); //Update board
        setTurn(data.turn);
      });
    return true;
  }

  /* Create new game on api and reset board */
  function newGame() {
    fetch("http://localhost:3001/new", {
      method: "POST",
    })
      .then((res) => res.json())
      .then((data) => {
        setFen(data.fen); //Update board
        setTurn(data.turn);
      });
  }

  /* Handle user input */
  function onDrop(sourceSquare: string, targetSquare: string) {
    let move = { from: sourceSquare, to: targetSquare };
    if (turn === "w") {
      sendMove(move);
    } else setFeedback("Not your turn!");
    return true;
  }

  return (
    <div className="flex-container">
      <div className="flex-content">
        <h2>{feedback}</h2>
        <Chessboard position={fen} onPieceDrop={onDrop} boardWidth={400} />
        <button
          onClick={() => {
            newGame();
          }}
        >
          New Game
        </button>
      </div>
    </div>
  );
}
