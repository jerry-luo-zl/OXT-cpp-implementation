def listAdd(l1, l2):
    return [x + y for x, y in zip(l1, l2)]

l1 = [1,2,3]
l2 = [4,5,6]
print(listAdd(l1, l2))