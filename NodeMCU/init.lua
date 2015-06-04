-- specify the SSID and PASSWORD for your AP 
ssid   = "Wifi-SSID" 
passwd = "Wifi-Password" 

--print ("Setting up WiFi...")
wifi.setmode(wifi.STATION) -- configure the device as a station (STA)
--print('MAC: ',wifi.sta.getmac())
--print('chip: ',node.chipid())
--print('heap: ',node.heap())

wifi.sta.config(ssid, passwd) 
wifi.sta.connect()
wifi.sta.autoconnect(1)
--print( 'Connecting WiFi...' )

retries = 0
tmr.alarm( 1, 1000, 1, function()
    ip = wifi.sta.getip() 
    if ip == nil then
        --print( "Retry #"..retries)
        if retries >= 10 then
            --print( 'WiFi connection failed!' ) 
            tmr.stop(1) -- stop timer 1
        else
            retries = retries + 1
        end
    else
        tmr.stop(1) -- stop timer 1
        --print( 'IP: ' .. ip ) 
        --print( 'MAC: ' .. wifi.sta.getmac() )
        status = wifi.sta.status() -- status=5 'STATION GOT IP' 
        dofile( 'send_card_to_server.lua' )
    end
end)
