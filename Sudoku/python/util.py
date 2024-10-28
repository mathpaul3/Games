from typing import Any, Iterable


def get_digit(number: int, base: int = 10) -> int:
    if base < 2:
        raise Exception("Base only can be an integer greater than 2 or equal to 2")
    digit = 1
    while int(number / base):
        digit += 1
        number = int(number / base)
    return digit


def flatten(obj: Any) -> list[Any]:
    try:
        result = list()
        for item in obj:
            if isinstance(item, Iterable) and not isinstance(item, str):
                result.extend(flatten(item))
            else:
                result.append(item)
        return result
    except TypeError:
        return [obj]
