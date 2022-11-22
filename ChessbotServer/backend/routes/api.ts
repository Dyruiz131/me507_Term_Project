import express, { Request, Response, Router } from "express";
import { Chess } from "chess.js";

const router: Router = express.Router();
let chess = new Chess();

// get board positions
router.get("/new", async (req: Request, res: Response) => {
  chess = new Chess();
  res.send("Gameboard cleared.");
});

// get board positions
router.get("/", async (req: Request, res: Response) => {
  res.send(chess.board());
});

router.get("/ascii", async (req: Request, res: Response) => {
  res.send(chess.ascii());
});

router.get("/move/:from/:to", async (req: Request, res: Response) => {
  const { from, to } = req.params;
  chess.move({ from, to });
  res.send(chess.board());
});

router.post("/move", async (req: Request, res: Response) => {
  const { from, to } = req.body;
  chess.move({ from, to });
  res.send(chess.board());
});

router.get("/fen", async (req: Request, res: Response) => {
  res.send({ fen: chess.fen() });
});

router.get("/status", async (req: Request, res: Response) => {
  res.send({ status: chess.isGameOver() });
});

// // create new recipe
// router.post("/", async (req: Request, res: Response) => {
//   // await Recipe.create(req.body);
//   res.send("Recipe created");
// });

// // update recipe
// router.put("/:name", async (req: Request, res: Response) => {
//   await Recipe.findOneAndUpdate({ name: req.params.name }, req.body);
//   res.send("Recipe updated");
// });

// // delete recipe
// router.delete("/:name", async (req: Request, res: Response) => {
//   await Recipe.findOneAndDelete({ name: req.params.name });
//   res.send("Recipe deleted");
// });

// // add ingredient to recipe
// router.put("/:recipeName/ingredient", async (req: Request, res: Response) => {
//   const { recipeName } = req.params;
//   const { newIngredient } = req.body;
//   const recipe = await Recipe.findOne({ name: recipeName });
//   if (recipe) {
//     recipe.ingredients = [...recipe.ingredients, newIngredient];
//     await recipe.save();
//     res.send("Ingredient added");
//   } else {
//     res.send("Failed to add ingredient");
//   }
// });

// // add instruction to recipe
// router.put("/:recipeName/instruction", async (req: Request, res: Response) => {
//   const { recipeName } = req.params;
//   const { newInstruction } = req.body;
//   const recipe = await Recipe.findOne({ name: recipeName });
//   if (recipe) {
//     recipe.instructions = [...recipe.instructions, newInstruction];
//     await recipe.save();
//     res.send("Instruction added");
//   } else {
//     res.send("Failed to add instruction");
//   }
// });

export default router;
