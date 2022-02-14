addEventListener("fetch", async event => {
    let i = 111;

    const queueingStrategy = new CountQueuingStrategy({ highWaterMark: 3 });
    const readable = new ReadableStream({
        start: function (controller) {
            controller.enqueue(i++)
        },
        pull: function (controller) {
            controller.enqueue(i++)
            if (i > 120) {
                controller.close()
            }
        },
        cancel: function (controller) {

        }
    }, queueingStrategy);

    let res = {
        "readable": readable.toString(),
        "highWaterMark": queueingStrategy.highWaterMark,
        "size": queueingStrategy.size(),
    }

    event.respondWith(new Response("hello" + " WebStreams !!\n" + JSON.stringify(res, null, 2)));
});