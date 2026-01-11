"""
CThaiNLP - Thai Natural Language Processing Library
Python bindings for the CThaiNLP C library
"""

__version__ = "0.1.0"
__author__ = "Wannaphong Phatthiyaphaibun"

from cthainlp.tokenize import word_tokenize
from cthainlp import newmm

__all__ = ["word_tokenize", "newmm", "__version__"]
