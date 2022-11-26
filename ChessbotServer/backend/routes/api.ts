import express, { Request, Response, Router } from "express";
import { Chess } from "chess.js";

const router: Router = express.Router();
let chess = new Chess();
let moveComplete: boolean = false;

// get board positions
router.post("/new", async (req: Request, res: Response) => {
  chess = new Chess();
  res.send({ fen: chess.fen() });
});

// get board positions
router.get("/", async (req: Request, res: Response) => {
  res.send({ board: chess.board(), turn: chess.turn() });
});

// get board positions in ascii
router.get("/ascii", async (req: Request, res: Response) => {
  res.send({ ascii: chess.ascii() });
});

// post move
router.post("/move", async (req: Request, res: Response) => {
  chess.move(req.body);
  res.send({ fen: chess.fen(), turn: chess.turn() });
});

// get board fen
router.get("/fen", async (req: Request, res: Response) => {
  res.send({ fen: chess.fen(), turn: chess.turn() });
});

// get board status
router.get("/status", async (req: Request, res: Response) => {
  res.send({ status: chess.isGameOver() });
});
export default router;
