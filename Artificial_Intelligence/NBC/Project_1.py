import csv
import matplotlib.pyplot as plt
import math

#############################################################################################################

# Task 1 : Feature Selection                                                                                #

#############################################################################################################

# data load
    # keys are stars and text. store in dictionary form

data = {}

with open('train.csv', 'r') as file:
    reader = csv.DictReader(file)
    for row in reader:
        for key, value in row.items():
            data.setdefault(key, []).append(value)

            
# For Task 3 
# using the range of count variable, we can control the number of training value   
'''
with open('train.csv', 'r') as file:
    count = 0
    reader = csv.DictReader(file)
    for row in reader:
        if(count >= 4000):
            break
        for key, value in row.items():
            data.setdefault(key, []).append(value)
            count += 0.5
'''

# set stop words
stop_words = []
with open('stopwords.txt', 'r') as file:
    stop_words = [line.strip() for line in file.readlines()]
    # add ''
stop_words.append('') 
    # stop_words list to set
stop_words = set(stop_words)  
                       
# Convert all text to lowercase
for i in range(len(data.get('text'))):
    data.get('text')[i] = data.get('text')[i].lower()

# Remove special characters
    # " !"#$%&'()*+,-./:;<=>?@[\]^_`{|}~" between double quote
def remove_chars(s):
    # add '\n'
    chars = "!\"#$%&\'()*+,-./:;<=>?@[\]^_`{|}~'\n'"
    for c in chars:
        if (c == '\n'):
            s = s.replace(c, ' ')
        else:
            s = s.replace(c, '')
    return s

for i in range(len(data.get('text'))):
    data.get('text')[i] = remove_chars(data.get('text')[i])
    
# Tokenize to words
    # store words in list
    # also store five stars words and one stars words separatly
words =[]
words_five = []
words_one = []
for i in range(len(data.get('text'))):
    words_temp = data.get('text')[i].split(" ")
    if(data.get('stars')[i] == '1'):
        words_one += words_temp
    else:
        words_five += words_temp

words = words_five + words_one

# Remove stop words
words = [word for word in words if word not in stop_words]
words_one = [word for word in words_one if word not in stop_words]  
words_five = [word for word in words_five if word not in stop_words]    

# store frequency
words_fre = {}
for item in words:
    if item in words_fre:
        words_fre[item] += 1
    else:
        words_fre[item] = 1

words_set = set(words)
        
words_five_fre = {}
for item in words_five:
    if item in words_five_fre:
        words_five_fre[item] += 1
    else:
        words_five_fre[item] = 1

words_five_fre_set = set(words_five)

words_one_fre = {}
for item in words_one:
    if item in words_one_fre:
        words_one_fre[item] += 1
    else:
        words_one_fre[item] = 1
        
words_one_fre_set = set(words_one)

words_common = words_one_fre_set.intersection(words_five_fre_set)

# Select 1,000 most frequently appeared words
sorted_words_fre = dict(sorted(words_fre.items(), key = lambda x: x[1], reverse=True))
print("Task 1 : 1,000 most frequently appeared words")
print(list(sorted_words_fre.items())[:1000])
print()

#############################################################################################################

# Task 2 : Model Training and Evaluation (Training Above / Tesing Below)                                                                   #

#############################################################################################################

# p(stars)
num_five = 0
num_one = 0
for i in range(len(data.get('stars'))):
    if(data.get('stars')[i] == '1'):
        num_one += 1
    else:
        num_five += 1

p_five = num_five / len(data.get('stars'))
p_one = num_one / len(data.get('stars'))

# test data
test_data = {}

with open('test.csv', 'r') as file:
    reader = csv.DictReader(file)
    for row in reader:
        for key, value in row.items():
            test_data.setdefault(key, []).append(value)
            
# Convert all text to lowercase
for i in range(len(test_data.get('text'))):
    test_data.get('text')[i] = test_data.get('text')[i].lower()
    
# Remove special characters
for i in range(len(test_data.get('text'))):
    test_data.get('text')[i] = remove_chars(test_data.get('text')[i])
    
    
TruePositive = 0
FalsePositive = 0
TrueNegative = 0
FalseNegative = 0

# Test
for i in range(len(test_data.get('text'))):
    actual_result = test_data.get('stars')[i]
    # Tokenize to words
    # words in list
    test_words =[]
    test_words = test_data.get('text')[i].split(" ")
    
    # Remove stop words
    test_words = [word for word in test_words if word not in stop_words]  

    # store frequency in dictionary
    test_words_fre = {}
    for item in test_words:
        if item in test_words_fre:
            test_words_fre[item] += 1
        else:
            test_words_fre[item] = 1

    p_five_word = 0
    p_one_word = 0
        
    for key in test_words_fre.keys():
        # Laplace Smoothing
        # search the words that both 5 stars and 1 stars have
        # preventing to be too small value, use logaric form
        if(key in words_five_fre_set and key in words_one_fre_set):
            p_five_word += math.log(((words_five_fre[key] + 1) / (len(words_five) + len(words_common))))
            p_one_word += math.log(((words_one_fre[key] + 1) / (len(words_one) + len(words_common))))
        
    p_five_word += math.log(p_five)
    p_one_word += math.log(p_one)
        
    temp_result = ''
    if(p_five_word > p_one_word):
        temp_result = '5'
    else:
        temp_result = '1'
        
    if(temp_result == '5' and actual_result =='5'):
        TruePositive += 1
    elif(temp_result == '1' and actual_result =='5'):
        FalseNegative += 1
    elif(temp_result == '5' and actual_result == '1'):
        FalsePositive += 1
    elif(temp_result == '1' and actual_result == '1'):
        TrueNegative += 1
    
Accuracy = (TrueNegative + TruePositive) / (TruePositive + FalsePositive + TrueNegative + FalseNegative)
print("Task 2 : Accuracy")
print("Accuracy =", Accuracy) 
print("TrueNegative =" , TrueNegative)
print("TruePositive =" ,TruePositive)
print("FalseNegative =" , FalseNegative)
print("FalsePositive =" ,FalsePositive)

#########################################################################################################

# Task 3 : Learning Curve Analysis                                                                      #

#########################################################################################################

# pre-calculated value 
# see line 26 ~ 34
x = ['10%', '30%', '50%', '70%', '100%']
y = [0.841, 0.87, 0.875, 0.885, 0.9]

plt.plot(x, y, marker='o')
plt.xlabel('Number of Training Data')
plt.ylabel('Accuracy')
plt.title('Learning Curve Analysis')
plt.show()