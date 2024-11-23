import bareos

fd = bareos.bsock.filedaemon.FileDaemon(address="::1", port=8888, password=bareos.util.Password("demo"), name="DIR-Test", dirname="DIR-Test")
print(fd.call("status").decode("utf-8"))
