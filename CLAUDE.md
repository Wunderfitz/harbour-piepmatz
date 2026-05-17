# harbour-piepmatz

SailfishOS mobile Twitter client built with Qt (C++ + QML). Migrating from Twitter API v1 to v2 incrementally.

## API v2 migration status

| Feature | Status |
|---|---|
| Home timeline (`/timelines/reverse_chronological`) | Done |
| Single tweet (`statuses/show.json` → `/2/tweets/:id`) | Done |
| Mentions timeline (`statuses/mentions_timeline.json` → `/2/users/:id/mentions`) | Done |
| Retweet timeline (`statuses/retweets_of_me.json` → `/2/users/:id/tweets`) | Done |

## Migration pattern

- v2 calls use `requestor->get(request, requestParameters)` with an `O0RequestParameter` list — the same OAuth-authenticated mechanism as the home timeline, not `manager->get()` with a bearer token header
- Secret identity support (`secretIdentityRequestor`) must be preserved: on error code 136 (blocked), retry with `secretIdentityRequestor->get()` and `HEADER_NO_RECURSION` to prevent infinite recursion. The ID must be extracted from the v2 URL path (last path segment) rather than from a query parameter as in v1
- v2 base URLs use `api.x.com`, constants defined in `twitterapi.h`
- v2 responses are normalized to v1-compatible `QVariantMap` structure via `normalizeTweetV2()`, `normalizeUserV2()`, `normalizeEntitiesV2()`, `normalizeMediaV2()` in `twitterapi.cpp`
- Default `max_results` for v2 timeline endpoints is `10`
- QML signal interface (`showStatusSuccessful`, etc.) and data format remain unchanged — no QML changes needed when migrating a backend call
- `getValidDate()` in `qml/js/functions.js` handles both ISO 8601 (v2) and v1 date formats
