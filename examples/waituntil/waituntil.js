addEventListener("fetch", async event => {
    event.waitUntil(new Promise(resolve => {
        setTimeout(() => {
            resolve();
        }, 1000);
    }));
    event.respondWith(new Response(" hello" + " worker !!"))
});