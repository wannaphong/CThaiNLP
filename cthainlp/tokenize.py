"""
Tokenization module for CThaiNLP
"""

import os
from typing import List, Optional

try:
    import _cthainlp
except ImportError:
    _cthainlp = None


def word_tokenize(
    text: str,
    engine: str = "newmm",
    custom_dict: Optional[str] = None,
) -> List[str]:
    """
    Segment Thai text into words.
    
    This is the main tokenization function, compatible with PyThaiNLP's API.
    
    Args:
        text (str): Input text to tokenize (UTF-8 encoded)
        engine (str): Tokenization engine. Currently only 'newmm' is supported.
                     Default is 'newmm'.
        custom_dict (str, optional): Path to custom dictionary file (one word per line).
                                     If None, uses the default dictionary.
    
    Returns:
        list: List of tokens (strings)
    
    Raises:
        RuntimeError: If tokenization fails
        ValueError: If unsupported engine is specified
        ImportError: If C extension is not properly installed
    
    Examples:
        >>> from cthainlp import word_tokenize
        >>> text = "ฉันไปโรงเรียน"
        >>> tokens = word_tokenize(text)
        >>> print(tokens)
        ['ฉัน', 'ไป', 'โรงเรียน']
        
        >>> # With custom dictionary
        >>> tokens = word_tokenize(text, custom_dict="path/to/dict.txt")
        
        >>> # Explicitly specify engine
        >>> tokens = word_tokenize(text, engine="newmm")
    """
    if _cthainlp is None:
        raise ImportError(
            "CThaiNLP C extension is not installed. "
            "Please install the package properly using: pip install -e ."
        )
    
    if engine != "newmm":
        raise ValueError(
            f"Unsupported engine '{engine}'. Currently only 'newmm' is supported."
        )
    
    if not isinstance(text, str):
        raise TypeError(f"text must be a string, got {type(text)}")
    
    # Handle empty string case
    if not text:
        return []
    
    # If custom_dict is provided and exists, use it; otherwise use None for default
    dict_path = None
    if custom_dict is not None:
        if not os.path.exists(custom_dict):
            raise FileNotFoundError(f"Dictionary file not found: {custom_dict}")
        dict_path = custom_dict
    
    # Call the C extension
    tokens = _cthainlp.segment(text, dict_path)
    
    return tokens


# Alias for compatibility
segment = word_tokenize
