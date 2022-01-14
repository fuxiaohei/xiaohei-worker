addEventListener('fetch', function (event) {
    event.respondWith(handleRequest(event.request))
})

async function handleRequest(request) {
    // Only GET requests work with this proxy.
    if (request.method !== 'GET') {
        return MethodNotAllowed(request)
    }
    fetch(`https://example.com`)
    return new Response("okkkkk!")
}

function MethodNotAllowed(request) {
    return new Response(`Method ${request.method} not allowed.`, {
        status: 405,
        headers: {
            'Allow': 'GET'
        }
    })
}