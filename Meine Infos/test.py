import bareos

fd = bareos.bsock.filedaemon.FileDaemon(address="::1", port=8888, password=bareos.util.Password("demo"), name="dummy_dir", dirname="dummy_dir")
print(fd.call("status").decode("utf-8"))
