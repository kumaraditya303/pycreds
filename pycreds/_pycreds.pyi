from typing import Dict, List, Optional

def get_password(service: str, account: str) -> str: ...
def set_password(service: str, account: str, password: str) -> bool: ...
def find_credentials(service: str) -> List[Dict[str, str]]: ...
def find_password(service: str) -> Optional[str]: ...
def delete_password(service: str, account: str) -> bool: ...
