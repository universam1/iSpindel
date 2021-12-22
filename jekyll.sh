#!/bin/sh
docker kill $(docker ps -q)
rm -fR _site
# docker run --rm -v $PWD:/usr/src/app -e JEKYLL_ENV=production -p 4000:4000 starefossen/github-pages:latest jekyll serve --config _config.yml --watch --host 0.0.0.0 --force_polling
docker run -p 35729:35729 -p 4000:4000 -p 5678:5678 -e JEKYLL_ENV=production -v $PWD:/site bretfisher/jekyll-serve

# docker run -it --entrypoint /bin/sh -v $(pwd):/site bretfisher/jekyll-serve