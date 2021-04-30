# -*- coding: utf-8 -*-


def _to_int(s):
    try:
        return int(s)
    except ValueError:
        return s


__version__ = "0.1.0"
version_info = tuple(_to_int(s) for s in __version__.split("."))
