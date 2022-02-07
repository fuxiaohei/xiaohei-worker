addEventListener("fetch", async event => {
    let i = 111;
    const readable = new ReadableStream({
        start:function(controller){
            controller.enqueue(i++)
        },
        pull:function(controller){
            controller.enqueue(i++)
            if(i>120){
                controller.close()
            }
        },
        cancel:function(controller){

        }
    });
    event.respondWith(new Response("hello" + " WebStreams !!" + readable));
});