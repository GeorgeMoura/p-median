from random import randrange

#MAGIC VALUES

#adj matrix random values will start at this value
init_matrix_element_range = 100
#adj matrix random values will finish at this value
final_matrix_element_range = 900
#the instance creation will start with this matrix size, always making a sum with itself
matrix_size_initializer = 100
#the number of instance groups that will be created
number_of_groups = 1
#the number of intances per group
number_of_instances = 3

def create_matrix(matrix_size):

	matrix = [[randrange(init_matrix_element_range, final_matrix_element_range) for i in range(matrix_size)] for j in range(matrix_size)]

	for idx, line in enumerate(matrix):
		line[idx] = 0

	return matrix

def p_value(matrix):
	return randrange(1, int(len(matrix)-1))

def set_file_name(group, element):
	string = "INST"+str(group+1)+"ELE"+str(element+1)+".txt"
	return string

def write_on_file(matrix, p, name):
	file = open(name, "w")
	file.write(str(len(matrix)))
	for i in matrix:
		file.write("\n")
		for j in i:
			file.write(str(j))
			file.write(" ")
	file.write("\n")
	file.write(str(p))
	file.close()
	
def main():

	matrix_size = 0

	for i in range(number_of_groups):
		matrix_size += matrix_size_initializer
		for j in range(number_of_instances):
			matrix = create_matrix(matrix_size)
			write_on_file(matrix, p_value(matrix), set_file_name(i,j))


main()
























