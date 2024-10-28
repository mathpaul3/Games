from sudoku import *

board = [
    [4, 0, 0, 0, 6, 0, 0, 7, 0],
    [0, 0, 0, 0, 0, 0, 6, 0, 0],
    [0, 3, 0, 0, 0, 2, 0, 0, 1],
    [7, 0, 0, 0, 0, 8, 5, 0, 0],
    [0, 1, 0, 4, 0, 0, 0, 0, 0],
    [0, 2, 0, 9, 5, 0, 0, 0, 0],
    [0, 0, 0, 0, 0, 0, 7, 0, 5],
    [0, 0, 9, 1, 0, 0, 0, 3, 0],
    [0, 0, 3, 0, 4, 0, 0, 8, 0],
]

if __name__ == "__main__":
    random.seed(0)

    # sudoku = Sudoku4x4()
    # sudoku = Sudoku6x6h()
    # sudoku = Sudoku6x6v()
    # sudoku = Sudoku8x8h()
    # sudoku = Sudoku8x8v()
    sudoku = Sudoku9x9(board=board)
    sudoku.activate_log().optimize()
    sudoku.solve(method="minimum_case_first")
    # sudoku = Sudoku12x12h()
    # sudoku = Sudoku12x12v()
    # sudoku = Sudoku16x16()
    # sudoku = Sudoku20x20h()
    # sudoku = Sudoku20x20v()
    sudoku = Sudoku25x25()
    # sudoku = Sudoku121x121()
    sudoku.activate_log().optimize()
    sudoku.solve(method="minimum_case_first")
