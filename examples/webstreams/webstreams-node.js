const {
    ReadableStream
} = require('stream/web');

let i = 111;
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
});

let reader = readable.getReader();

(async () => {
    for (; ;) {
        let data = await reader.read()
        console.log(data)
        if (data.done) {
            break
        }
    }
})();
