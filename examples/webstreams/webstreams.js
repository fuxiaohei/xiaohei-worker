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

        },
        // type: "bytes"
    }, queueingStrategy);

    const reader = readable.getReader();

    let res = {
        "readable": readable.toString(),
        "locked": readable.locked,
        "highWaterMark": queueingStrategy.highWaterMark,
        "size": queueingStrategy.size(),
        "reader": reader.toString(),
        "read_chunk": await reader.read()
    }

    event.respondWith(new Response("hello" + " WebStreams !!\n" + JSON.stringify(res, null, 2)));
});