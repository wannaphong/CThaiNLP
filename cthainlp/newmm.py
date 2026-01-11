"""
newmm module for CThaiNLP
Provides the newmm tokenization engine compatible with PyThaiNLP's API
"""

from typing import List, Optional


def segment(
    text: Optional[str],
    custom_dict: Optional[str] = None,
    keep_whitespace: bool = True,
) -> List[str]:
    """
    Segment Thai text using the newmm algorithm.
    
    This function provides PyThaiNLP-compatible API for newmm tokenization.
    
    Args:
        text (str, optional): Input text to tokenize. Can be None.
        custom_dict (str, optional): Path to custom dictionary file.
        keep_whitespace (bool): Whether to keep whitespace in the output.
                               Default is True.
    
    Returns:
        list: List of tokens (strings). Returns empty list if text is None or empty.
    
    Examples:
        >>> from cthainlp.tokenize import newmm
        >>> newmm.segment("ฉันไปโรงเรียน")
        ['ฉัน', 'ไป', 'โรงเรียน']
        
        >>> newmm.segment(None)
        []
        
        >>> newmm.segment("")
        []
    """
    # Handle None and empty string
    if text is None or text == "":
        return []
    
    # Import here to avoid circular imports
    from cthainlp.tokenize import word_tokenize
    
    # Call word_tokenize with appropriate parameters
    return word_tokenize(
        text,
        engine="newmm",
        custom_dict=custom_dict,
        keep_whitespace=keep_whitespace,
    )
