.. csv-table::
   :header: "configuration directive name", "type of data", "default value", "remark"

   :config:option:`fd/filedaemon/AbsoluteJobTimeout`\          , "= PINT32", "", ""
   :config:option:`fd/filedaemon/AllowBandwidthBursting`\      , "= BOOLEAN", "no", ""
   :config:option:`fd/filedaemon/AllowedJobCommand`\           , "= STRING_LIST", "", ""
   :config:option:`fd/filedaemon/AllowedScriptDir`\            , "= DIRECTORY_LIST", "", ""
   :config:option:`fd/filedaemon/AlwaysUseLmdb`\               , "= BOOLEAN", "no", ""
   :config:option:`fd/filedaemon/Compatible`\                  , "= BOOLEAN", "no", ""
   :config:option:`fd/filedaemon/Description`\                 , "= STRING", "", ""
   :config:option:`fd/filedaemon/FdAddress`\                   , "= ADDRESS", "9102", ""
   :config:option:`fd/filedaemon/FdAddresses`\                 , "= ADDRESSES", "9102", ""
   :config:option:`fd/filedaemon/FdPort`\                      , "= PORT", "9102", ""
   :config:option:`fd/filedaemon/FdSourceAddress`\             , "= ADDRESS", "0", ""
   :config:option:`fd/filedaemon/HeartbeatInterval`\           , "= TIME", "0", ""
   :config:option:`fd/filedaemon/LmdbThreshold`\               , "= PINT32", "", ""
   :config:option:`fd/filedaemon/LogTimestampFormat`\          , "= STRING", "", ""
   :config:option:`fd/filedaemon/MaximumBandwidthPerJob`\      , "= SPEED", "", ""
   :config:option:`fd/filedaemon/MaximumConcurrentJobs`\       , "= PINT32", "20", ""
   :config:option:`fd/filedaemon/MaximumConnections`\          , "= PINT32", "42", ""
   :config:option:`fd/filedaemon/MaximumNetworkBufferSize`\    , "= PINT32", "", ""
   :config:option:`fd/filedaemon/Messages`\                    , "= RES", "", ""
   :config:option:`fd/filedaemon/Name`\                        , "**= NAME** ", "", "**required** "
   :config:option:`fd/filedaemon/PidDirectory`\                , "= DIRECTORY", "/var/lib/bareos *(platform specific)*", ""
   :config:option:`fd/filedaemon/PkiCipher`\                   , "= ENCRYPTION_CIPHER", "aes128", ""
   :config:option:`fd/filedaemon/PkiEncryption`\               , "= BOOLEAN", "no", ""
   :config:option:`fd/filedaemon/PkiKeyPair`\                  , "= DIRECTORY", "", ""
   :config:option:`fd/filedaemon/PkiMasterKey`\                , "= DIRECTORY_LIST", "", ""
   :config:option:`fd/filedaemon/PkiSignatures`\               , "= BOOLEAN", "no", ""
   :config:option:`fd/filedaemon/PkiSigner`\                   , "= DIRECTORY_LIST", "", ""
   :config:option:`fd/filedaemon/PluginDirectory`\             , "= DIRECTORY", "", ""
   :config:option:`fd/filedaemon/PluginNames`\                 , "= PLUGIN_NAMES", "", ""
   :config:option:`fd/filedaemon/ScriptsDirectory`\            , "= DIRECTORY", "", ""
   :config:option:`fd/filedaemon/SdConnectTimeout`\            , "= TIME", "1800", ""
   :config:option:`fd/filedaemon/SecureEraseCommand`\          , "= STRING", "", ""
   :config:option:`fd/filedaemon/SubSysDirectory`\             , "*= DIRECTORY* ", "", "*deprecated* "
   :config:option:`fd/filedaemon/TlsAllowedCn`\                , "= STRING_LIST", "", ""
   :config:option:`fd/filedaemon/TlsAuthenticate`\             , "= BOOLEAN", "no", ""
   :config:option:`fd/filedaemon/TlsCaCertificateDir`\         , "= DIRECTORY", "", ""
   :config:option:`fd/filedaemon/TlsCaCertificateFile`\        , "= DIRECTORY", "", ""
   :config:option:`fd/filedaemon/TlsCertificate`\              , "= DIRECTORY", "", ""
   :config:option:`fd/filedaemon/TlsCertificateRevocationList`\ , "= DIRECTORY", "", ""
   :config:option:`fd/filedaemon/TlsCipherList`\               , "= DIRECTORY", "", ""
   :config:option:`fd/filedaemon/TlsDhFile`\                   , "= DIRECTORY", "", ""
   :config:option:`fd/filedaemon/TlsEnable`\                   , "= BOOLEAN", "yes", ""
   :config:option:`fd/filedaemon/TlsKey`\                      , "= DIRECTORY", "", ""
   :config:option:`fd/filedaemon/TlsRequire`\                  , "= BOOLEAN", "no", ""
   :config:option:`fd/filedaemon/TlsVerifyPeer`\               , "= BOOLEAN", "no", ""
   :config:option:`fd/filedaemon/VerId`\                       , "= STRING", "", ""
   :config:option:`fd/filedaemon/WorkingDirectory`\            , "= DIRECTORY", "/var/lib/bareos *(platform specific)*", ""
