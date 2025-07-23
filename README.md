<p align="left" style="display: flex; align-items: center;">
  <img src="https://raw.githubusercontent.com/pavelkral/freelander/refs/heads/main/resource/icons/micon.png" width="64" height="64" style="vertical-align: middle; margin-right: 10px;">
  <span style="font-size: 20px; line-height: 64px; vertical-align: middle;"><strong>Nadpis vedle obrázku</strong></span>
</p>

<table style="border: none; border-collapse: collapse;">
  <tr>
     <td style="border: none; padding-right: 12px;">
      <img src="https://raw.githubusercontent.com/pavelkral/freelander/refs/heads/main/resource/icons/micon.png" style="width: 48px; height: 48px; object-fit: cover;">
      </td>
       <td style="border: none; ">
      <h1 style="margin: 0;"> Freelander </h1>
    </td>
  </tr>
</table>

Alternative for Rainlendar desktop calendar widget which use Google Calendar api.

![Image](https://github.com/pavelkral/Freelander/raw/main/media/freelander2.png)


> [!WARNING]
> This application is still in development. I wrote it for myself because Google Calendar api is just in pro version which is not free. 
> It is not feature complete and it is not tested on all platforms. 


## Download Binaries

- Windows: [freelander-win64.zip](https://github.com/pavelkral/Freelander/releases/tag/Alpha)
- Linux: *in progress*

## Build from Source

### Build Dependencies

- Qt (min 5.15), Qt 6 is recommended

### Build Steps camke
```
$ mkdir build
$ cd build
$ cmake --build . -- -j8
```
### Build Steps qmake
```
$ qmake
$ make
```


### Run
```
Update your Google id in config.json in root directory and start app.
```

<p align=center>
  <img src="https://github.com/pavelkral/Freelander/raw/main/media/freelander-alpha.png">
</p>

## References

- https://qt.io
- https://developers.google.com/workspace/calendar/api/guides/overview
