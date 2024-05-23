import pandas as pd
import matplotlib.pyplot as plt
from matplotlib.dates import DateFormatter

# Specify the Excel file path
ExcelFilePath = "D:\College\CSE_2_Term_2\Automatic Control\Project\Dual-Axis-Solar-Tracker\Scripting\Data.xlsx"

# Read data from Excel sheet
Sheet = pd.read_excel(ExcelFilePath)

# Extract data from specific columns
time = pd.to_datetime(Sheet['Time'], format='%H:%M:%S')
current = list(Sheet['Current']) 
voltage = list(Sheet['Voltage'])  

# Calculate power
power = []
for v, c in zip(voltage, current):
    power.append(v * c)

# Create the plot
plt.figure(figsize=(10, 6))
plt.plot(time, power, marker='o', linestyle='-', color='r')

# Adding title and labels
plt.title('Solar Panel Power')
plt.xlabel('Time')
plt.ylabel('Power (Watts)')

# Formatting the x-axis for time
time_format = DateFormatter('%H:%M:%S')
plt.gca().xaxis.set_major_formatter(time_format)

# Show the plot
plt.grid(True)
plt.show()