function send_card(nfc_card_id)
    conn=net.createConnection(net.TCP, 0)
    conn:on("receive", function(conn, payload) 
        --print(payload)        
        i,j = string.find(payload,"{")
        k,l = string.find(payload,"}")
        if i == nil then
        else
            if k == nil then
            else
                payload = string.sub(payload,i,k)
                uart.write(0,payload)
            end
        end
        
        
    end)
    conn:on("disconnection", function(conn)
        --print("Got disconnection...")
    end) 
    
    conn:connect(80,'XX.XX.XX.XX') 
    conn:send("GET /<--Path to Server-->/" ..nfc_card_id.. " HTTP/1.1\r\n")
    conn:send("Host: XX.XX.XX.XX\r\n")
    conn:send("Connection: keep-alive\r\n")
    conn:send("Accept: */*\r\n")
    conn:send("User-Agent: Mozilla/4.0 (compatible; esp8266 Lua; Windows NT 5.1)\r\n")
    conn:send("\r\n")
end

uart.setup(0,9600,8,0,1,0)
uart.on("data","\n", function(data)
    data = string.sub(data,1,8)
    send_card(data)
    --uart.write(0,data)
    data = nil
end, 0)

