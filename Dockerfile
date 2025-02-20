# Use an official Ubuntu image as the base for the build stage
FROM ubuntu:22.04 AS build

# Install necessary build tools
RUN apt-get update && \
    apt-get install -y \
    cmake \
    g++ \
    git \
    make \
    python3 \
    python3-pip \
    && rm -rf /var/lib/apt/lists/*

# Copy the entire project into the container
COPY . /usr/src/ChessEngine

# Set the working directory
WORKDIR /usr/src/ChessEngine

# Create a build directory and compile the project
RUN mkdir build && \
    cd build && \
    cmake .. && \
    make

# Create a minimal runtime image
FROM ubuntu:22.04 AS runtime

# Install Python and pip in the runtime stage
RUN apt-get update && \
    apt-get install -y \
    python3 \
    python3-pip \
    && rm -rf /var/lib/apt/lists/*

# Install Python dependencies for lichess-bot
COPY requirements.txt /usr/src/ChessEngine/requirements.txt
WORKDIR /usr/src/ChessEngine
RUN pip3 install -r requirements.txt

# Copy the compiled executable and lichess-bot files from the build stage
COPY --from=build /usr/src/ChessEngine/build/ChessEngine /usr/local/bin/ChessEngine
COPY --from=build /usr/src/ChessEngine/lichess-bot /usr/src/ChessEngine/lichess-bot

# Set the working directory to the lichess-bot directory
WORKDIR /usr/src/ChessEngine/lichess-bot

# Set the entry point to run lichess-bot
ENTRYPOINT ["python3", "lichess-bot.py"]