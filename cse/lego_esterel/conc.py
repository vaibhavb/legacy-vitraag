import sys

while 1:
    l = sys.stdin.readline ()
    if l == "":
        break
    while l [-2:] == "\\\n":
        l2 = sys.stdin.readline ()
        if l2 == "":
            break
        l = l[:-2] + l2
    sys.stdout.write (l)


