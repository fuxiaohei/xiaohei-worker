addEventListener("fetch", async event => {
    const readable = new ReadableStream();
    event.respondWith(new Response("hello" + " WebStreams !!" + readable));
});