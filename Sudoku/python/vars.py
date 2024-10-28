from typing import Literal

COLOR: dict[
    Literal[
        "DEFAULT",
        "BLACK",
        "RED",
        "GREEN",
        "YELLOW",
        "BLUE",
        "MAGENTA",
        "CYAN",
        "WHITE",
        "BRIGHT BLACK",
        "BRIGHT RED",
        "BRIGHT GREEN",
        "BRIGHT YELLOW",
        "BRIGHT BLUE",
        "BRIGHT MAGENTA",
        "BRIGHT CYAN",
        "BRIGHT WHITE",
    ],
    str,
] = {
    key: "\033[" + str(value) + "m"
    for key, value in {
        "DEFAULT": 0,
        "BLACK": 30,
        "RED": 31,
        "GREEN": 32,
        "YELLOW": 33,
        "BLUE": 34,
        "MAGENTA": 35,
        "CYAN": 36,
        "WHITE": 37,
        "BRIGHT BLACK": 90,
        "BRIGHT RED": 91,
        "BRIGHT GREEN": 92,
        "BRIGHT YELLOW": 93,
        "BRIGHT BLUE": 94,
        "BRIGHT MAGENTA": 95,
        "BRIGHT CYAN": 96,
        "BRIGHT WHITE": 97,
    }.items()
}

SUDOKU_TYPE = Literal[
    "4x4",
    "6x6h",
    "6x6v",
    "8x8h",
    "8x8v",
    "9x9",
    "12x12h",
    "12x12v",
    "16x16",
    "20x20h",
    "20x20v",
    "25x25",
    "jigsaw",
    "greater than",
    "consecutive",
    "non-consecutive",
    "diagonal",
    "killer",
    "even-odd",
    "extra region",
    "windoku",
    "samurai",
    "3d",
    "calcudoku",
]

LOG_LEVEL = Literal["debug", "info"]
