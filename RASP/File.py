from dataclasses import dataclass

@dataclass
class File :
  file_name_size : int
  file_name : str
  file_size : int
  bytes_written : int

def print_file(file : File):
  print(f"file_name_size: {file.file_name_size}")
  print(f"file_name: {file.file_name}")

def open_file(file : File, type : str):
  try:
    with open(file.file_name, type) as f:
      return f
  except FileNotFoundError:
    print(f"Arquivo {file.file_name} não encontrado.")