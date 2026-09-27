import ctypes

file = open('./wordBank.txt')
file2 = open('./stringtotranslate.txt')
wordBank = file.read()
stringToTranslate = file2.read()

lib = ctypes.CDLL('./translate.so')

lib.PyTranslateStory.argtypes = [ctypes.c_char_p, ctypes.c_char_p]
lib.PyTranslateStory.restype = ctypes.c_char_p

print((lib.PyTranslateStory(stringToTranslate.encode("utf-8"), wordBank.encode("utf-8"))).decode("utf-8"))

