import numpy as np
import pandas as pd

df = pd.DataFrame([['a' ,'b', 'c'], [1, 2, 3]], columns=['hana', 'dool', 'set'])

df.to_csv('./data/test.csv', index=False)