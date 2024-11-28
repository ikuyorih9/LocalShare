from Header import *
from uart import *
# from functions import *

ser = setup()

while True:
  header = get_header(ser)
  print_header(header)