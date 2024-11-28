from dataclasses import dataclass

@dataclass
class Header :
    function : int
    user_name_size : int
    user_name : str
    password_size : int
    password : str

    def __post_init__(self):
        if not (0 <= self.function <= 255):
            raise ValueError("function deve estar no intervalo de 0 a 255")
        if not (0 <= self.user_name_size <= 255):
            raise ValueError("user_name_size deve estar no intervalo de 0 a 255")
        if not (0 <= self.password_size <= 255):
            raise ValueError("password_size deve estar no intervalo de 0 a 255")
        
def print_header(header : Header):
    print(f"function: {header.function}")
    print(f"user_name_size: {header.user_name_size}")
    print(f"user_name: {header.user_name}")
    print(f"password_size: {header.password_size}")
    print(f"password: {header.password}")