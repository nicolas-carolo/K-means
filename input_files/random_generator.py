import random


def main():
    min_value = -1000
    max_value = 1000
    coordinate_dimension = 115
    n_coordinates = 50000
    f = open("input.csv","w+")
    for i in range(n_coordinates):
        for j in range(coordinate_dimension):
            random_value = random.uniform(min_value, max_value)
            if j < coordinate_dimension - 1:
                f.write('{:1.6e}'.format(round(random_value, 10)) + ",")
            else:
                f.write('{:1.6e}'.format(round(random_value, 10)) + "\n")
    f.close()


if __name__ == '__main__':
    main() 
