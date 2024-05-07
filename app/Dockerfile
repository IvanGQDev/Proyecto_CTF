FROM node:20.11.1-alpine
WORKDIR /app
COPY package.json /app
RUN npm i
COPY index.js /app/
EXPOSE 80
CMD ["node", "index.js"]