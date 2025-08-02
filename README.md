<table>
  <tr>
    <td>
      <img src="https://raw.githubusercontent.com/pavelkral/freelander/refs/heads/main/resource/icons/micon.png" width="48" height="48">
    </td>
    <td>
      <h2> Freelander </h2>
    </td>
  </tr>
</table>

Alternative for Rainlendar desktop calendar widget which use Google Calendar api.

![Image](https://raw.githubusercontent.com/pavelkral/freelander/refs/heads/main/media/freelander2.png)


> [!WARNING]
> This application is still in development. I wrote it for myself because Google Calendar api is just in pro version which is not free. 
> It is not feature complete and it is not tested on all platforms. 


## Download Binaries

- Windows: [freelander-win64.zip](https://github.com/pavelkral/Freelander/releases/tag/Alpha)
- Linux: *in progress*

## Build from Source

### Build Dependencies

- Qt6
- CMAKE

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
  <img src="https://raw.githubusercontent.com/pavelkral/freelander/refs/heads/main/media/freelander-alpha.png">
  <img src="https://raw.githubusercontent.com/pavelkral/freelander/refs/heads/main/media/freelander1.png">
</p>



## References

- https://qt.io
- https://developers.google.com/workspace/calendar/api/guides/overview
