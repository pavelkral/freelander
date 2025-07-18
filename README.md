# Freelander 

Alternative for Rainlendar calendar widget which use Google Calendar api.

<p align=center>
  <img src="https://github.com/pavelkral/Freelander/raw/main/media/freelander-alpha.png">
</p>

> [!WARNING]
> This application is still in development.I it wrote for myself because Google Calendar api is just in pro version which is not free .

It is not feature complete and it is not tested on all platforms. 


## Download Binaries

- Windows: [freelander-alpha-win64.zip](https://github.com/pavelkral/Freelander/releases/tag/Alpha)
- Linux: *in progress*

## Build from Source

### Build Dependencies

- Qt (min version 5.15), Qt 6

### Build Steps
```
mkdir build
cd build
cmake .. -G "Ninja" -DCMAKE_PREFIX_PATH="C:/Qt/6.9/msvc2019_64" -DCMAKE_BUILD_TYPE=Release
cmake --build . -- -j8
```


### Run
```
Update your Google id in config.json in root directory and start app.
```





![Image](https://github.com/pavelkral/Freelander/raw/main/media/freelander2.png)


## References

- https://qt.io
- https://developers.google.com/workspace/calendar/api/guides/overview
