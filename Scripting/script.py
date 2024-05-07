'''
Libraries Section
'''
import pandas
import matplotlib.pyplot as plt
from sqlalchemy import create_engine

engine = create_engine("excel:///?Excel File='C:/MyExcelWorkbooks/SampleWorkbook.xlsx'")

df = pandas.read_sql("SELECT Name, Revenue FROM Sheet WHERE Name = 'Bob'", engine)

df.plot(kind="bar", x="Name", y="Revenue")
plt.show()



