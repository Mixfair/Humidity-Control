uart.setup(0,9600,8,0,1,0)
uart.on("data","\r", function(data)  
  
    if global_c~=nil then  
        uart.write(1,"from stm32: "..data)  
        global_c:send(data) 
        global_c:close()
        global_c = nil 
    end   
end, 0) 
 
sv=net.createServer(net.TCP, 333)
global_c = nil 
sv:listen(9999, function(c)
    if global_c~=nil then
        global_c:close()
    end  
    global_c=c
    c:on("connection", function(sck, c)
         
    end)
    c:on("disconnection", function(sck, c)
        global_c = nil
    end)
    c:on("receive",function(sck,pl) 
        global_c = sck
         
        uart.write(0,pl.."\r")
    end)
end) 
