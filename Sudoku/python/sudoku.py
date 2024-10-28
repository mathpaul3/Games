from abc import ABCMeta, abstractmethod
from functools import reduce
from queue import Queue
import random
import time
from typing import Literal

from data import INIT_BOARD
from util import flatten, get_digit
from vars import COLOR, SUDOKU_TYPE, LOG_LEVEL


class Group:
    def __init__(self, name: str, cells: list[int], init_available: int):
        self.name: str = name
        self.cells: list[int] = sorted(cells)
        self.available: int = reduce(
            lambda acc, cur: acc + (1 << cur), range(len(cells)), 0
        )
        self.AVAILABLE: int = init_available

    def disable(self, number: int) -> "Group":
        if number < 1:
            return
        self.available &= ~(self.AVAILABLE & (1 << (number - 1)))
        return self

    def enable(self, number: int) -> "Group":
        if number < 1:
            return
        self.available |= 1 << (number - 1)
        return self

    def __getitem__(self, idx: int) -> int:
        return self.cells[idx]

    def __str__(self) -> str:
        return f"available: {
            "".join([(str(idx+1) if idx+1<10 else chr(ord('a')+idx-9)) if self.available & (1 << idx) else '0' for idx in range(len(self.cells))])
            }, {self.name}: [{', '.join(map(str, self.cells))}]"


class SudokuBase(metaclass=ABCMeta):
    def __init__(
        self,
        sudoku_type: SUDOKU_TYPE,
        N: int,
        board: list[list[int]] | list[int] | None = None,
    ):
        self._N: int = N
        self._type: SUDOKU_TYPE = sudoku_type
        self._INIT_BOARD: list[int] = list()
        self._board: list[int] = list()
        self._log: bool = False
        self._log_level: LOG_LEVEL = "info"
        self._optimized: bool = False
        self._group: list[Group] = list()
        self._ref: list[list[Group]] = [[] for _ in self._board]
        self.__init_board(board)

    def activate_log(
        self, activate: bool = True, log_level: LOG_LEVEL = "info"
    ) -> "SudokuBase":
        self._log = activate
        self._log_level = log_level
        return self

    def optimize(self, optimized: bool = True) -> "SudokuBase":
        self._optimized = optimized
        return self

    def put(self, idx: int, value: int) -> bool:
        self._board[idx] = value
        if not self.__is_valid_cell(idx):
            return False
        for group in self._ref[idx]:
            group.disable(value)
        return True

    def solve(
        self,
        method: Literal["minimum_case_first", "brute_forcing"] = "minimum_case_first",
    ) -> None:
        self._board = self._INIT_BOARD.copy()
        if self._log:
            print(self)
            start = time.time()
        if method == "brute_forcing":
            result = self.__solve_brute_forcing(0, self._board[0])
        elif method == "minimum_case_first":
            result = self.__solve_minimum_case_first(0, self._board[0])
        if not result:
            print("Unsolvable")
        if self._log:
            end = time.time()
            print(f"{end - start:.2f}sec")

    def __solve_brute_forcing(self, init_idx: int, init_number: int) -> bool:
        self._board[init_idx] = init_number
        for group in self._ref[init_idx]:
            group.disable(init_number)

        fill: set[int] = set()
        queue: Queue[tuple[int, int]] = Queue()
        for idx, cell in enumerate(self._board):
            if cell:
                continue
            available_numbers = self._get_available_numbers(idx)
            if len(available_numbers) == 1:
                queue.put((idx, available_numbers[0]))
            elif len(available_numbers) == 0:
                self._delete_cell(init_idx)
                return False

        while not queue.empty():
            idx, available_number = queue.get()
            if self._board[idx]:
                continue

            fill.add(idx)
            if not self.put(idx, available_number):
                for cell in fill:
                    self._delete_cell(cell)
                self._delete_cell(init_idx)
                return False

            if self._log and self._log_level == "debug":
                self._update_print()

            for group in self._ref[idx]:
                group.disable(available_number)

            for group in self._ref[idx]:
                for ref_cell in group:
                    if not self._board[ref_cell]:
                        available_numbers = self._get_available_numbers(ref_cell)
                        if len(available_numbers) == 1:
                            queue.put((ref_cell, available_numbers[0]))

        if self._log and self._log_level == "info":
            self._update_print()
        if not self.__count_empty_cell():
            return True

        for idx, cell in enumerate(self._board):
            if not cell:
                available_numbers = self._get_available_numbers(idx)
                if len(available_numbers):
                    for number in available_numbers:
                        if self.__solve_brute_forcing(idx, number):
                            return True
                    for idx in fill:
                        self._delete_cell(idx)
                    self._delete_cell(init_idx)
                    return False

    def __solve_minimum_case_first(self, init_idx: int, init_number: int) -> bool:
        self._board[init_idx] = init_number
        for group in self._ref[init_idx]:
            group.disable(init_number)

        fill: set[int] = set()
        queue: Queue[tuple[int, int]] = Queue()
        for idx, cell in enumerate(self._board):
            if cell:
                continue
            available_numbers = self._get_available_numbers(idx)
            if len(available_numbers) == 1:
                queue.put((idx, available_numbers[0]))
            elif len(available_numbers) == 0:
                self._delete_cell(init_idx)
                return False

        while not queue.empty():
            idx, available_number = queue.get()
            if self._board[idx]:
                continue

            fill.add(idx)
            if not self.put(idx, available_number):
                for cell in fill:
                    self._delete_cell(cell)
                self._delete_cell(init_idx)
                return False

            if self._log and self._log_level == "debug":
                self._update_print()

            for group in self._ref[idx]:
                group.disable(available_number)

            for group in self._ref[idx]:
                for ref_cell in group:
                    if not self._board[ref_cell]:
                        available_numbers = self._get_available_numbers(ref_cell)
                        if len(available_numbers) == 1:
                            queue.put((ref_cell, available_numbers[0]))
        if self._log and self._log_level == "info":
            self._update_print()
        if not self.__count_empty_cell():
            return True

        min_possibility = [self._N + 1, None]  # (possible_case, idx)
        for idx, cell in enumerate(self._board):
            if not cell:
                available_numbers = self._get_available_numbers(idx)
                if len(available_numbers) < min_possibility[0]:
                    min_possibility[0], min_possibility[1] = len(available_numbers), idx
                    if len(available_numbers) == 2:
                        break
        available_numbers = self._get_available_numbers(min_possibility[1])
        for number in available_numbers:
            if self.__solve_minimum_case_first(min_possibility[1], number):
                return True
        for idx in fill:
            self._delete_cell(idx)
        self._delete_cell(init_idx)
        return False

    def __init_board(self, board: list[list[int]] | list[int] | None = None) -> None:
        if board:
            self._board = flatten(board)
        else:
            self._make_random_board()
        self._make_group()
        if not self.__is_valid_board():
            raise Exception("Invalid Board Provided")
        self._INIT_BOARD = self._board.copy()

    def __is_valid_cell(self, idx: int) -> bool:
        if not self._board[idx]:
            for group in self._ref[idx]:
                if not group.available:
                    return False
            return True
        for group in self._ref[idx]:
            for ref_cell in group:
                if ref_cell != idx and self._board[ref_cell] == self._board[idx]:
                    return False
        return True

    def __is_valid_board(self) -> bool:
        if not self._is_valid_board_length():
            return False
        for cell in self._board:
            if not self.__is_valid_cell(cell):
                return False
        return True

    def __count_empty_cell(self) -> int:
        return reduce(lambda acc, cur: acc + 1 if not cur else acc, self._board, 0)

    @abstractmethod
    def _delete_cell(self, idx: int) -> None:
        pass

    @abstractmethod
    def _get_available_numbers(self, idx: int) -> list[int]:
        pass

    @abstractmethod
    def _is_valid_board_length(self) -> bool:
        pass

    @abstractmethod
    def _make_group(self) -> None:
        pass

    @abstractmethod
    def _make_random_board(self) -> None:
        pass

    @abstractmethod
    def _shuffle_board(self) -> None:
        pass

    @abstractmethod
    def _update_print(self) -> None:
        pass

    @abstractmethod
    def __str__(self) -> str:
        pass


class SudokuNxN(SudokuBase):
    def __init__(
        self,
        sudoku_type: SUDOKU_TYPE,
        N: int,
        hgN: int,
        vgN: int,
        board: list[list[int]] | list[int] | None = None,
    ):
        """
        sudoku_type: defines type of sudoku\n
        hgN: defines how many horizontal groups in a vertical line\n
        vgN: define how many vertical groups in a horizontal line\n
        """
        if hgN * vgN != N:
            raise Exception("Invalid N, hgN, vgN")
        self._hgN: int = hgN
        self._vgN: int = vgN
        self._N: int = N
        self.AVAILABLE = reduce(lambda acc, cur: acc + (1 << cur), range(self._N), 0)
        super().__init__(sudoku_type, N, board)

    def _delete_cell(self, idx: int):
        for group in self._ref[idx]:
            for cell in group:
                if cell != idx and self._board[cell] == self._board[idx]:
                    break
            else:
                group.enable(self._board[idx])
        self._board[idx] = 0

    def _get_available_numbers(self, idx: int) -> list[int]:
        if self._board[idx]:
            return []
        available_numbers = self.AVAILABLE
        for group in self._ref[idx]:
            available_numbers &= group.available
        for group in self._ref[idx]:
            if self._optimized and group.name[0] == "g":
                global_available_numbers = 0
                for cell in group:
                    if self._board[cell] or cell == idx:
                        continue
                    local_available_numbers = self.AVAILABLE
                    for ref_group in self._ref[cell]:
                        local_available_numbers &= ref_group.available
                    global_available_numbers |= local_available_numbers

                numbers = [
                    num + 1
                    for num in range(self._N)
                    if available_numbers & (~global_available_numbers) & (1 << num)
                ]
                if len(numbers) == 1:
                    return numbers
        return [num + 1 for num in range(self._N) if available_numbers & (1 << num)]

    def _is_valid_board_length(self) -> bool:
        return len(self._board) == self._N * self._N

    def _make_group(self) -> None:
        self._ref = [[] for _ in range(self._N * self._N)]
        self._group = list()
        for idx in range(self._N):
            self._group.append(
                Group(
                    f"v{idx}",
                    [idx + _ * self._N for _ in range(self._N)],
                    self.AVAILABLE,
                )
            )
            self._group.append(
                Group(
                    f"h{idx}",
                    [idx * self._N + _ for _ in range(self._N)],
                    self.AVAILABLE,
                )
            )
            self._group.append(
                Group(
                    f"g{idx}",
                    self.__make_NxN_group(idx),
                    self.AVAILABLE,
                )
            )
        for group in self._group:
            for cell in group:
                group.disable(self._board[cell])
                self._ref[cell].append(group)

    def __make_NxN_group(self, idx: int) -> list[int]:
        return list(
            map(
                int,
                [
                    (idx // self._vgN) * (self._N * int(self._N / self._hgN))
                    + (idx % self._vgN) * int(self._N / self._vgN)
                    + ridx * self._N
                    + cidx
                    for cidx in range(int(self._N / self._vgN))
                    for ridx in range(int(self._N / self._hgN))
                ],
            )
        )

    def _make_random_board(self) -> None:
        if self._type in INIT_BOARD:
            self._board = flatten(INIT_BOARD[self._type])
        else:
            self._board = [0 for _ in range(self._N * self._N)]
        self._shuffle_board()
        sample = random.sample(
            [_ for _ in range(len(self._board))],
            random.randint(int(self._N * self._N * 0.4), int(self._N * self._N * 0.6)),
        )
        self._board = [
            self._board[idx] if idx in sample else 0 for idx in range(len(self._board))
        ]

    def _shuffle_board(self) -> None:
        self.__shuffle_board_number()
        self.__shuffle_board_horizontal()
        self.__shuffle_board_vertical()

    def __shuffle_board_horizontal(self) -> None:
        """
        shuffle rows
        """
        row = [r for r in range(int(self._N / self._hgN))]
        for hgidx in range(self._hgN):
            random.shuffle(row)
            visited = [False for _ in range(len(row))]
            for r in range(len(row)):
                if row[r] == r or visited[r]:
                    continue
                cur = r
                row_to_be_changed = [
                    self._board[
                        (hgidx * int(self._N / self._hgN) + row[cur]) * self._N + cidx
                    ]
                    for cidx in range(self._N)
                ]
                while not visited[cur]:
                    visited[cur] = True
                    for cidx in range(self._N):
                        tmp = self._board[
                            (hgidx * int(self._N / self._hgN) + cur) * self._N + cidx
                        ]
                        self._board[
                            (hgidx * int(self._N / self._hgN) + cur) * self._N + cidx
                        ] = row_to_be_changed[cidx]
                        row_to_be_changed[cidx] = tmp
                    cur = row.index(cur)

    def __shuffle_board_vertical(self) -> None:
        """
        shuffle columns
        """
        col = [c for c in range(int(self._N / self._vgN))]
        for vgidx in range(self._vgN):
            random.shuffle(col)
            visited = [False for _ in range(len(col))]
            for c in range(len(col)):
                if col[c] == c or visited[c]:
                    continue
                cur = c
                column_to_be_changed = [
                    self._board[
                        cidx * self._N + (vgidx * int(self._N / self._vgN) + col[cur])
                    ]
                    for cidx in range(self._N)
                ]
                while not visited[cur]:
                    visited[cur] = True
                    for cidx in range(self._N):
                        tmp = self._board[
                            cidx * self._N + (vgidx * int(self._N / self._vgN) + cur)
                        ]
                        self._board[
                            cidx * self._N + (vgidx * int(self._N / self._vgN) + cur)
                        ] = column_to_be_changed[cidx]
                        column_to_be_changed[cidx] = tmp

                    cur = col.index(cur)

    def __shuffle_board_number(self) -> None:
        """
        shuffle numbers
        """
        num = [n + 1 for n in range(self._N)]
        random.shuffle(num)

        num_idx = [
            [idx for idx, cell in enumerate(self._board) if (cell == n + 1)]
            for n in range(self._N)
        ]
        for number in range(self._N):
            if (number + 1) == num[number]:
                continue
            for idx in num_idx[number]:
                self._board[idx] = num[number]

    def _update_print(self) -> None:
        print("\033[A" * (self._N + self._hgN + 1))
        print(self)
        # time.sleep(1 / (self.N * self.N))

    def __str__(self) -> str:
        ret = ""
        for row in range(self._N):
            for col in range(self._N):
                ret += (
                    (
                        COLOR["BRIGHT BLACK"]
                        if self._INIT_BOARD[row * self._N + col] != 0
                        else ""
                    )
                    + format(self._board[row * self._N + col], f"{get_digit(self._N)}d")
                    + (COLOR["DEFAULT"] if self._INIT_BOARD != 0 else "")
                    + (" " if self.__vertical_div(col) else "|")
                )
            ret += "\n"
            if self.__horizontal_div(row):
                ret += (("-" * (get_digit(self._N) + 1)) * self._N)[:-1] + "\n"
        return ret

    def __vertical_div(self, col: int) -> bool:
        return (
            col % (self._N / self._vgN) != (self._N / self._vgN) - 1
            or col == self._N - 1
        )

    def __horizontal_div(self, row: int) -> bool:
        return (
            row % (self._N / self._hgN) == (self._N / self._hgN) - 1
            and row != self._N - 1
        )


class Sudoku4x4(SudokuNxN):
    def __init__(self, board: list[list[int]] | list[int] | None = None):
        super().__init__("4x4", 4, 2, 2, board)


class Sudoku6x6h(SudokuNxN):
    def __init__(self, board: list[list[int]] | list[int] | None = None):
        super().__init__("6x6h", 6, 3, 2, board)


class Sudoku6x6v(SudokuNxN):
    def __init__(self, board: list[list[int]] | list[int] | None = None):
        super().__init__("6x6v", 6, 2, 3, board)


class Sudoku8x8h(SudokuNxN):
    def __init__(self, board: list[list[int]] | list[int] | None = None):
        super().__init__("8x8h", 8, 4, 2, board)


class Sudoku8x8v(SudokuNxN):
    def __init__(self, board: list[list[int]] | list[int] | None = None):
        super().__init__("8x8v", 8, 2, 4, board)


class Sudoku9x9(SudokuNxN):
    def __init__(self, board: list[list[int]] | list[int] | None = None):
        super().__init__("9x9", 9, 3, 3, board)


class Sudoku12x12h(SudokuNxN):
    def __init__(self, board: list[list[int]] | list[int] | None = None):
        super().__init__("12x12h", 12, 4, 3, board)


class Sudoku12x12v(SudokuNxN):
    def __init__(self, board: list[list[int]] | list[int] | None = None):
        super().__init__("12x12v", 12, 3, 4, board)


class Sudoku16x16(SudokuNxN):
    def __init__(self, board: list[list[int]] | list[int] | None = None):
        super().__init__("16x16", 16, 4, 4, board)


class Sudoku20x20h(SudokuNxN):
    def __init__(self, board: list[list[int]] | list[int] | None = None):
        super().__init__("20x20h", 20, 5, 4, board)


class Sudoku20x20v(SudokuNxN):
    def __init__(self, board: list[list[int]] | list[int] | None = None):
        super().__init__("20x20v", 20, 4, 5, board)


class Sudoku25x25(SudokuNxN):
    def __init__(self, board: list[list[int]] | list[int] | None = None):
        super().__init__("25x25", 25, 5, 5, board)


class Sudoku121x121(SudokuNxN):
    def __init__(self, board: list[list[int]] | list[int] | None = None):
        super().__init__("121x121", 121, 11, 11, board)
