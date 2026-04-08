import gensim.downloader as api
embedding = api.load("glove-wiki-gigaword-50")
print(embedding['king'])