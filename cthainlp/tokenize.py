"""
Tokenization module for CThaiNLP
"""

import os
from typing import List, Optional

try:
    import _cthainlp
except ImportError:
    _cthainlp = None


def _get_default_dict_path() -> str:
    """
    Get the default dictionary file path.
    
    Returns:
        str: Absolute path to the default dictionary file
    """
    # Get the directory where this module is located
    module_dir = os.path.dirname(os.path.abspath(__file__))
    
    # Try to find dictionary in package data directory
    # When installed: cthainlp/data/thai_words.txt
    dict_path = os.path.join(module_dir, "data", "thai_words.txt")
    if os.path.exists(dict_path):
        return dict_path
    
    # Try parent directory (development mode)
    # When in source: CThaiNLP/data/thai_words.txt
    parent_dir = os.path.dirname(module_dir)
    dict_path = os.path.join(parent_dir, "data", "thai_words.txt")
    if os.path.exists(dict_path):
        return dict_path
    
    # Fallback: return None to use hardcoded dictionary
    return None


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
            "Please install the package properly using: pip install cthainlp"
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
    
    # Determine which dictionary to use
    if custom_dict is not None:
        # User provided a custom dictionary
        if not os.path.exists(custom_dict):
            raise FileNotFoundError(f"Dictionary file not found: {custom_dict}")
        dict_path = custom_dict
    else:
        # Use default dictionary
        dict_path = _get_default_dict_path()
    
    # Call the C extension
    tokens = _cthainlp.segment(text, dict_path)
    
    return tokens


# Alias for compatibility
segment = word_tokenize
