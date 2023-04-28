import pandas as pd
import wget
import subprocess
import os

# read top 1000 populated cities
df = pd.read_csv("uscities.csv", nrows=1000)

# remove unnecessary columns
df = df.loc[:, ['city', 'state_name', 'lat', 'lng', 'population', 'timezone']]

# check for empty rows and remove
df = df.dropna(how='all')

# remove Puerto Rico/Hawaii/Alaska data
df = df[~(df['state_name'].str.contains('Puerto Rico', case=False) | df['state_name'].str.contains('Hawaii', case=False) | df['state_name'].str.contains('Alaska', case=False))]

# dir to store bin data
if not os.path.exists('bin'):
    os.mkdir('bin')
# if exists and contains files remove them to prevent name indexing for new files
else:
    file_list = os.listdir('bin')
    for file_name in file_list:
        os.remove(os.path.join('bin', file_name))

# download data

# temperature
wget.download('ftp://tgftp.nws.noaa.gov/SL.us008001/ST.expr/DF.gr2/DC.ndfd/AR.conus/VP.001-003/ds.temp.bin', 'bin/temp_1_3.bin')
wget.download('ftp://tgftp.nws.noaa.gov/SL.us008001/ST.expr/DF.gr2/DC.ndfd/AR.conus/VP.004-007/ds.temp.bin', 'bin/temp_4_7.bin')
# chance rain
wget.download('ftp://tgftp.nws.noaa.gov/SL.us008001/ST.expr/DF.gr2/DC.ndfd/AR.conus/VP.001-003/ds.pop12.bin', 'bin/chance_rain_1_3.bin')
wget.download('ftp://tgftp.nws.noaa.gov/SL.us008001/ST.expr/DF.gr2/DC.ndfd/AR.conus/VP.004-007/ds.pop12.bin', 'bin/chance_rain_4_7.bin')
# snow amount
wget.download('ftp://tgftp.nws.noaa.gov/SL.us008001/ST.expr/DF.gr2/DC.ndfd/AR.conus/VP.001-003/ds.snow.bin', 'bin/snow.bin')
# wind direction
wget.download('ftp://tgftp.nws.noaa.gov/SL.us008001/ST.expr/DF.gr2/DC.ndfd/AR.conus/VP.001-003/ds.wdir.bin', 'bin/wind_dir_1_3.bin')
wget.download('ftp://tgftp.nws.noaa.gov/SL.us008001/ST.expr/DF.gr2/DC.ndfd/AR.conus/VP.004-007/ds.wdir.bin', 'bin/wind_dir_4_7.bin')
# wind speed
wget.download('ftp://tgftp.nws.noaa.gov/SL.us008001/ST.expr/DF.gr2/DC.ndfd/AR.conus/VP.001-003/ds.wspd.bin', 'bin/wind_spd_1_3.bin')
wget.download('ftp://tgftp.nws.noaa.gov/SL.us008001/ST.expr/DF.gr2/DC.ndfd/AR.conus/VP.004-007/ds.wspd.bin', 'bin/wind_spd_4_7.bin')
# visibility
wget.download('ftp://tgftp.nws.noaa.gov/SL.us008001/ST.expr/DF.gr2/DC.ndfd/AR.conus/VP.001-003/ds.vis.bin', 'bin/vis.bin')
# relative humidity
wget.download('ftp://tgftp.nws.noaa.gov/SL.us008001/ST.expr/DF.gr2/DC.ndfd/AR.conus/VP.001-003/ds.maxrh.bin', 'bin/rel_humid_1_3.bin')
wget.download('ftp://tgftp.nws.noaa.gov/SL.us008001/ST.expr/DF.gr2/DC.ndfd/AR.conus/VP.004-007/ds.maxrh.bin', 'bin/rel_humid_4_7.bin')

# get data for each city 1-3 / 4-7 days 
bin_files = ["bin/" + filename + ".bin" for filename in ["temp_1_3", "temp_4_7", "chance_rain_1_3", "chance_rain_4_7", "snow", "wind_dir_1_3",
                                                        "wind_dir_4_7", "wind_spd_1_3", "wind_spd_4_7", "vis", "rel_humid_1_3", "rel_humid_4_7"]]
rows = []
rows_dict = {}
# iterate through bin files
for i, bin_file in enumerate(bin_files):

    # iterate through data frame rows
    for index, row in df.iterrows():
        lat, lng= row['lat'], row['lng']
        # decode bin file and capture output
        command = ["degrib"] + [bin_file] + ["-P", "-pnt", f"{lat}, {lng}"]
        output = subprocess.check_output(command, text=True)

        # prepare new rows to be added to final df
        for j in range(1, len(output.strip().split('\n'))):
            fields = output.strip().split('\n')[j].split(',')
            valid_time = fields[-2]

            row_dict = {
                'city': row['city'],
                'state_name': row['state_name'],
                'lat': lat,
                'lng': lng,
                'population': row['population'],
                'timezone': row['timezone'],
                'validTime': valid_time,
            }

            # depending on bin file append cooresponding fields
            if i == 0 or i == 1:
                row_dict.update({
                    'temp': fields[-1]
                })
            elif i == 2 or i == 3:
                row_dict.update({
                    'chance_rain' : fields[-1]
                })
            elif i == 4:
                row_dict.update({
                    'snow_amt' : fields[-1]
                })
            elif i == 5 or i == 6:
                row_dict.update({
                    'wind_dir' : fields[-1]
                })
            elif i == 7 or i == 8:
                row_dict.update({
                    'wind_spd' : fields[-1]
                })
            elif i == 9:
                row_dict.update({
                    'vis' : fields[-1]
                })
            elif i == 10 or i == 11:
                row_dict.update({
                    'rel_humid' : fields[-1]
                })
        
            rows.append(row_dict)
    

rows_df = pd.DataFrame(rows, index=None)
rows_df = rows_df.sort_values(['city', 'validTime'], ascending=[True, True])
rows_df.fillna(method='ffill', inplace=True)
rows_df.fillna(method='bfill', inplace=True)
rows_df.drop_duplicates(subset=['city', 'state_name', 'validTime'], keep='first', inplace=True)
rows_df.to_csv('weather_data.csv', index=False)
    