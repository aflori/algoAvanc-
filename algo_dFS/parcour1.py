import sys
import random
import time
import os
import copy

# size_x, size_y = [int(x) for x in input().split()]
# labyrinthe = [ [c for c in input()] for i in range(size_y) ]
size_input = input("") # two numbers awaited

size_x, size_y = size_input.split()
size_x :int = int(size_x)
size_y :int = int(size_y)

labyrinthe: list[list] = []
for i in range(size_y): #we are awaiting size_y lines
	line_i = input()

	labyrinthe.append([]) # add a line to the labyrinthe
	for character in line_i:
		labyrinthe[i].append(character) # add, case



class Position:
	def __init__(self, x, y):
		self.x = x
		self.y = y

	def __repr__(self):
		return "({}, {})".format(self.x, self.y)

	def __str__(self):
		return self.__repr__()


def print_labyrinthe(replace_number_per_R= True):
	str_labyrinthe = copy.deepcopy(labyrinthe)
	if replace_number_per_R:
		for line in str_labyrinthe:
			for i in range(len(line)):
				if type(line[i])==int:
					line[i] = 'R'
		str_labyrinthe = [ "".join(array) for array in str_labyrinthe]
	else:
		for i in range(len(str_labyrinthe)):
			line_str = ""
			line_array = str_labyrinthe[i]

			for c in str_labyrinthe[i]:
				if type(c) == int:
					if c<10: #less than 1 digit
						line_str+= "  " + str(c) + " "
					elif c<100:
						line_str+= " " + str(c) + " "
					else:
						line_str+= " " + str(c)
				else:
					line_str+= "  " + c + " "

			str_labyrinthe[i] = line_str
	str_labyrinthe = "\n".join(str_labyrinthe).replace('M', '.')
	print(str_labyrinthe)

def get_start_position():
	for i in range(0, size_y):
		for j in range(0, size_x):
			if labyrinthe[i][j] == 'S':
				labyrinthe[i][j] = '.'
				return Position(j,i)
	return None

def isNotOnGoal(position):
	return labyrinthe[position.y][position.x] != 'G'

def isValidPosition(pos):
	return pos.x>=0 and pos.y>=0 and pos.x<size_x and pos.y<size_y

def isInEmptyCase(pos):
	return labyrinthe[pos.y][pos.x] in ['.', 'G']

def get_next_position(position_actu):
	potentialPosition = [
		Position(position_actu.x, position_actu.y-1),
		Position(position_actu.x+1, position_actu.y),
		Position(position_actu.x-1, position_actu.y),
		Position(position_actu.x, position_actu.y+1),
	]
	random.shuffle(potentialPosition)
	for p in potentialPosition:
		if isValidPosition(p) and isInEmptyCase(p):
			return p
	return None

clear = lambda: os.system('clear')

def print_step(position):
	clear()
	old_value = labyrinthe[position.y][position.x]

	labyrinthe[position.y][position.x] = 'm'
	print_labyrinthe(False)
	# print()
	
	labyrinthe[position.y][position.x] = old_value
	time.sleep(0.5)

position = get_start_position()
ariane_deplacement = [  ]
number_movement = 0
if position==None:
	print("Le personnage n'est pas placé dans le labyrinthe", file= sys.stderr)
	exit(1)
try:
# if True:
	while isNotOnGoal(position):

		labyrinthe[position.y][position.x] = number_movement
		print_step(position)

		next_position = get_next_position(position)
		number_movement+=1
		
		if next_position == None:
			next_position = ariane_deplacement.pop()
		else:
			ariane_deplacement.append(position)

		position = next_position

	ariane_deplacement.append(position)
except Exception:
	print("Pas de chemins possible", file=sys.stderr)
	exit(1)


clear()

print(f"pour un total de {number_movement} movements et a trouvé un chemin de {len(ariane_deplacement)} déplacements:")
print(ariane_deplacement)

print("soit:")
initial_coord = ariane_deplacement[0]
labyrinthe[initial_coord.y][initial_coord.x] = 'S'
for i in range(2, len(ariane_deplacement)):
	pos1, pos2, pos3 = ariane_deplacement[i-2:i+1]
	
	if pos1.x == pos2.x == pos3.x:
		c = '│'
	elif pos1.y == pos2.y == pos3.y:
		c = '─'
	elif pos1.y < pos2.y or pos3.y < pos2.y:
	
		if pos1.x>pos2.x or pos3.x>pos2.x:
			c = '└'
		else:
			c = '┘'

	else:
		if pos1.x>pos2.x or pos3.x>pos2.x:
			c = '┌'
		else:
			c = '┐'
	labyrinthe[pos2.y][pos2.x] = c

print_labyrinthe()